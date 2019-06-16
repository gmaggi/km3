#ifndef KM3_H5_CONVETER_INCLUDED
#define KM3_H5_CONVETER_INCLUDED

#include <boost/variant/variant.hpp>
#include <boost/variant/get.hpp>

#include <iostream>
#include <string>
#include <map>
#include <set>
#include <utility>

#include "KM3H5RegisteredDataType.hh"
#include "KM3H5InserterDataType.hh"
#include "KM3H5Wrappers.hh"
#include "KM3H5InfoFromInput.hh"

namespace H5Converter
{
  class KM3H5Converter
  {
  private:
    
    typedef boost::variant<PODWrapper<unsigned int>, 
			   PODWrapper<int>, 
			   PODWrapper<double>, 
			   ParticleWrapper>  Variant_t;
			   
    typedef std::vector<Variant_t>  VariantVector_t;
    typedef std::map<std::pair<std::string,std::string>,VariantVector_t> VariantVectorMap_t;
    
    //In ctor list
    std::string infilename_;
    std::string outfilename_;
    
    //others
    std::unique_ptr<H5::H5File> file_;
    
    VariantVectorMap_t mapOfTables_;
    
    std::size_t n_pushedBackCounter_;

    template <class T>
    void
    FillTable(std::unique_ptr<H5::DataSet> &OutDataSet,
	      H5::Group &group,
	      const VariantVector_t &variantVect,
	      const std::string membername) 
    {
      std::vector<T> noVariantVect;
      
      for (VariantVector_t::const_iterator variant= variantVect.begin();
	   variant !=variantVect.end(); variant++){
	
	const T &noVariant = boost::get<T>(*variant);
	noVariantVect.push_back(noVariant);
      }
      const std::size_t rank = 1;
      const hsize_t dim[] = { noVariantVect.size() };
      const H5::DataSpace space(rank,dim);

      std::shared_ptr<H5::CompType> comptype = 
	KM3H5InserterDataType::InsertMemberInH5CompType(noVariantVect);

      OutDataSet.reset(new H5::DataSet( group.createDataSet(membername, *comptype, space) ) );
      OutDataSet->write(noVariantVect.data(), *comptype);
    }

    template <typename T, typename D>
    void ReFillTable(const std::pair<std::string, std::string> tableName,
		     const D t)
    {
      VariantVectorMap_t::iterator itMap = mapOfTables_.find( tableName );

      boost::get<T>( itMap->second.back() );
      const std::size_t size = n_pushedBackCounter_ - itMap->second.size() ;
            
      std::size_t pos=0;
      if ( itMap->second.size()!=1 ){ pos= n_pushedBackCounter_-1; }

      VariantVector_t::iterator posIt = itMap->second.begin(); 
      std::advance(posIt,pos);

      const auto &wrapper = WrapperForH5::PrePopulateTable(t,false);

      mapOfTables_[tableName].insert(posIt,size,wrapper);
    }
    
    void SetFileInfo(const std::vector<pair<string,string> > &stringPairs, 
		     const std::string table_name)
    {
      using std::string;
           
      const std::size_t rank = 1;
      const hsize_t dims[rank]={1};
      const H5::DataSpace dataspace(rank, dims);            

      const std::array<int,rank> dummyTable = {0};

      const H5::DataType datatype(H5::PredType::NATIVE_INT);
      H5::DataSet dataset = file_->createDataSet(table_name, datatype, dataspace);
      dataset.write( dummyTable.data(), datatype );
      
      //attributes
      const H5::DataSpace attrDataspace(H5S_SCALAR);

      for (std::vector<std::pair<string,string> >::const_iterator it = stringPairs.begin();
	   it!= stringPairs.end(); ++it){
	
	const string second=it->second;

	if ( second.empty() ) continue;

	const H5::StrType strType(H5::PredType::C_S1, second.length());
	H5::Attribute attri = dataset.createAttribute(it->first,strType,attrDataspace);
	attri.write(strType, second);
	attri.close();
      }
      dataset.close();

    }

    
  public:
    KM3H5Converter()
    {}

    
    KM3H5Converter(const std::string infilename,
		   const std::string outfilename):
      infilename_(infilename),
      outfilename_(outfilename)
    {
      file_.reset(new H5::H5File(outfilename, H5F_ACC_TRUNC ));
      n_pushedBackCounter_=0;
    }

    virtual ~KM3H5Converter()
    {} 
   

    void WriteIOFileNamesAsAttr()
    {
      const std::vector<pair<std::string,std::string> > &fileInfo = 
	{ std::make_pair("INFILE",infilename_), 
	  std::make_pair("OUTFILE",outfilename_) };
      
      this->SetFileInfo(fileInfo,"file_info");
    }


    void WriteMetaFromInputFileAsAttr()
    {
      if ( not infilename_.empty() ){
	
	const std::vector<pair<std::string,std::string> > &metas = 
	  KM3H5InfoFromInput::GetMetaFromInputFile(infilename_);
	
	this->SetFileInfo(metas,"meta");
      }
      else{
	std::cout<<"Meta data from input File will not be written since no input file is given"<<std::endl;
      } 
    }


    template <class T>
    void PushBack(const T member,
		  const std::pair<std::string,std::string> memberPair) 
    {
      const auto &wrapper = WrapperForH5::PrePopulateTable(member);

      mapOfTables_[memberPair].push_back(wrapper);
    }

    template <class T>
    void PushBack(const T member,
		  std::string group,
		  std::string table)
    {
      const auto &wrapper = WrapperForH5::PrePopulateTable(member);

      mapOfTables_[std::make_pair(group,table)].push_back(wrapper);
    }


    void CheckNcallsForPushBack()
    {
      n_pushedBackCounter_++; 

      for (VariantVectorMap_t::iterator initMapIt = mapOfTables_.begin(); 
	   initMapIt != mapOfTables_.end(); initMapIt++){
	
	const std::pair<std::string,std::string> tableName = initMapIt->first;
	VariantVectorMap_t::const_iterator itMap = mapOfTables_.find( tableName );
	
	if ( itMap == mapOfTables_.end() ){
	  throw std::invalid_argument("Some thing wrong; map of tables has no group: "+tableName.first
				      +", and Data Set: "+tableName.second+", at "+__PRETTY_FUNCTION__); 
	}
	
	const std::size_t tableSize = itMap->second.size();
	
	if ( tableSize < n_pushedBackCounter_ ){
	 
	  try { this->ReFillTable<PODWrapper<int> >(tableName,int()); }
	  catch (const std::exception& e){}

	  try { this->ReFillTable<PODWrapper<double> >(tableName,double()); }
	  catch (const std::exception& e){}

	  try { unsigned int any=0; this->ReFillTable<PODWrapper<unsigned int> >(tableName,any); }
	  catch (const std::exception& e){}

	  try { this->ReFillTable<ParticleWrapper>(tableName,JFIT::JFit()); }
	  catch (const std::exception& e){}
	  
	}

	if ( tableSize > n_pushedBackCounter_ ){
	  throw std::out_of_range("Some thing wrong in "+std::string(__PRETTY_FUNCTION__)+
	  			  ", perhaps more than one PushBack call for the same element");
	}

      }
    }
    
      
    void WriteTableForGeneratedInfo(const std::pair<std::string,std::string> &pair)
    {
      using std::string;

      const GeneratedInfo &genInfo = 
	KM3H5InfoFromInput::PreputateGeneratedInfoTable(infilename_);

      const std::size_t rank = 1;
      const hsize_t dims[rank]={1};
      const H5::DataSpace dataSpace(rank, dims);            
      const std::array<GeneratedInfo,rank> genTable = {genInfo};

      std::shared_ptr<H5::CompType> comptype = 
	KM3H5InserterDataType::InsertGeneratedInfoInH5CompType(); 

      const string groupName   = pair.first;
      const string dataSetName = pair.second;

      H5::Group group;
      try        { group = { file_->openGroup(groupName)   }; }
      catch (...){ group = { file_->createGroup(groupName) }; } 

      H5::DataSet dataSet= group.createDataSet(dataSetName, *comptype, dataSpace);

      dataSet.write( genTable.data(), *comptype);

      //attributes:
      const std::vector<std::pair<string, string> > &stringPairs=
	KM3H5InfoFromInput::PrepopulateGeneratorInfoAttributes(infilename_);

      const H5::DataSpace attrDataspace(H5S_SCALAR);

      for (std::vector<std::pair<string,string> >::const_iterator it = stringPairs.begin();
	   it!= stringPairs.end(); ++it)
	{
	  const string second=it->second;
	  const H5::StrType strType(H5::PredType::C_S1, second.length());
	  H5::Attribute attri = dataSet.createAttribute(it->first,strType,attrDataspace);
	  attri.write(strType, second);
	  attri.close();
	}
      dataSet.close();
      group.close();
    }


    void WriteTables() 
    {
      if ( not n_pushedBackCounter_ ){
	throw std::runtime_error("You Have to call CheckNcallsForPushBack() after the last PushBack");
      }

      std::cout<<"WRITE TABLES WITH LENGTH "<<n_pushedBackCounter_<<std::endl;     

      std::set<std::string> setGroups;
      for (VariantVectorMap_t::const_iterator mapIt = mapOfTables_.begin(); 
	   mapIt != mapOfTables_.end(); mapIt++){
	
	const std::string groupName= mapIt->first.first;
	setGroups.insert(groupName);
      }

      for (std::set<std::string>::iterator setGroup = setGroups.begin(); 
	   setGroup != setGroups.end(); ++setGroup){

	H5::Group group;
	try        { group = { file_->openGroup(*setGroup)   }; }
	catch (...){ group = { file_->createGroup(*setGroup) }; } 
	
	for (VariantVectorMap_t::const_iterator mapIt=mapOfTables_.begin(); 
	     mapIt != mapOfTables_.end(); mapIt++){

	  const std::string mapGroup   = mapIt->first.first;

	  if ( mapGroup!=*setGroup ) continue;

	  const std::string membername = mapIt->first.second;
	  const VariantVector_t &variantVec = mapIt->second;
	  
	  std::cout<<"Writting Table for:  "<<membername<<" in Group: "<<*setGroup<<std::endl;
	  
	  std::unique_ptr<H5::DataSet> dataset;

	  try{
	    this->FillTable<ParticleWrapper>(dataset, group, variantVec, membername);
	  }
	  catch (const std::exception& e){}
	  try{
	    this->FillTable<PODWrapper<double> >(dataset, group, variantVec, membername);
	  }
	  catch (const std::exception& e){}   
	  try{
	    this->FillTable<PODWrapper<unsigned int> >(dataset, group, variantVec, membername);
	  }
	  catch (const std::exception& e){}   
	  try{
	    this->FillTable<PODWrapper<int> >(dataset, group, variantVec, membername);
	  }
	  catch (const std::exception& e){}
	 
	  dataset->close();
	}
	group.close();
      }
    }


    void CloseFile() 
    {
      std::cout<<"Closing File"<<std::endl;
      file_->close();
    }
    
  };
}
#endif
