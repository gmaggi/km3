import numpy as np
import weightedstats as ws
import tables
import math, warnings

class AnalysisL0(object):
    def __init__(self,
                 inputFileName=None,
                 relational=None, 
                 cut_value=None, 
                 var_to_cut=None,
                 **kwargs):

        self._angularMedians = None
        self._angularMeans = None

        self.infile      = tables.open_file(inputFileName)

        tableName        = kwargs.get('table_name','neutrino')
        self.mc_particle = self.infile.root.mc.__getattr__(tableName)

        self.spectrum    = kwargs.get('spectrum',2.0)
        if self.spectrum < 0.0:
            warnings.warn('neutrino spectrum should be positive, you are passing nu_spectrum= {0}'\
                          .format(self.spectrum))
        
        use_weight       = kwargs.get('use_weight',True)
        if use_weight: 
            self.w1   = self.infile.root.mc.w1.cols.value[:]              
        else:
            shape        = len(self.infile.root.Event.runNumber.cols.value[:])
            self.w1   = np.ones(shape,dtype=np.float) 

        
        if isinstance(var_to_cut,list):
            list_bools=[]
            for v, c, r in zip(var_to_cut,cut_value,relational):
                table,attr = v.split('.')
                variable   = self.infile.root.reco.__getattr__(table).cols.__getattribute__(attr)[:] 
                b          = r(variable,c)
                list_bools.append(b)
            
            self.bool_cut = np.logical_and.reduce(np.array(list_bools))
        elif not var_to_cut is None:
            table,attr    = var_to_cut.split('.')
            variable      = self.infile.root.reco.__getattr__(table).cols.__getattribute__(attr)[:] 
            cut           = relational(variable,cut_value)
            self.bool_cut = cut
        else:
            shape         = len(self.mc_particle.cols.dir_z[:])
            self.bool_cut = np.ones(shape,dtype=np.bool) # <- No cuts

            
    @staticmethod
    def _resolution(truetable,fittable,cut):

        recoEfficiency = (len(fittable.cols.status[:][cut]) / float( len(fittable.cols.status[:]) ))*100
        print 'Recontructed Efficiency: {0}'.format(recoEfficiency)

        dotp= (truetable.cols.dir_x[:][cut]*fittable.cols.dir_x[:][cut]+
               truetable.cols.dir_y[:][cut]*fittable.cols.dir_y[:][cut]+
               truetable.cols.dir_z[:][cut]*fittable.cols.dir_z[:][cut])
        
        physicsDotp = np.where(dotp[:]<1.0,dotp[:],1.0)
            
        resolution  = np.arccos(physicsDotp[:])                    
        return np.degrees(resolution)[:]


    def _angular_resolution_perEbin(self,recofitname,**kwargs):
        print 'Using {0} table in _angular_resolution_perEbin '.format(recofitname)
        n_steps    = kwargs.get('n_steps',15)

        if recofitname=='KinematicAngle':
            fit   = self.infile.root.mc.mc_muon
        else:
            fit   = self.infile.root.reco.__getattr__(recofitname)

        cut = np.logical_and.reduce((fit.cols.status[:]==1,fit.cols.is_in_event[:]==1,self.bool_cut))

        allResolutions  = self._resolution(self.mc_particle,fit,cut)

        resolutions = allResolutions[:]
        energies    = self.mc_particle.cols.energy[:][cut]
        weights     = self.w1[:][cut]*np.power(energies,-self.spectrum)
        
        minEne  = np.log10( np.min(energies[:]) )
        maxEne  = np.log10( np.max(energies[:]) )
        binSize = (maxEne-minEne) / n_steps
     
        spaces    = np.linspace(minEne,maxEne,n_steps,endpoint=False)
        lowerBins = 10**np.array(spaces)
        upperBins = 10**np.array(spaces+binSize)

        mean_binned_resolutions=[]
        median_binned_resolutions=[]
        energy_bins=[]
        arrayOfArrayReolution=[]

        print 'Global angular resolution for {0} is: {1}: '.format(recofitname,
                                                                   ws.numpy_weighted_median(resolutions,weights))

        for min_en,max_en in zip(lowerBins, upperBins):
            
            energy_range   = np.logical_and(energies>=min_en,energies<max_en)       
            energy         = energies[energy_range]
            binned_weights = weights[energy_range]
            
            binned_resolutions= resolutions[energy_range]
       
            if len(binned_resolutions)==0:
                warnings.warn('Empty energy-bin in range: {0} to {1}'.format(binned_resolutions.min(),
                                                                             binned_resolutions.max()))
                continue
     
            weighted_bin_res     = ws.numpy_weighted_median(binned_resolutions,binned_weights)
            weighted_bin_resMean = ws.numpy_weighted_mean(binned_resolutions,binned_weights)
         
            median_binned_resolutions.append(weighted_bin_res)
            mean_binned_resolutions.append(weighted_bin_resMean)
            energy_bins.append(math.log((min_en+max_en)/2.0,10))
           
        return np.array(energy_bins), np.array(median_binned_resolutions), np.array(mean_binned_resolutions) 

    @property
    def angular_median(self):
        return self._angularMedians

    @angular_median.setter
    def angular_median(self,values):
        medians  = self._angular_resolution_perEbin(values[0], n_steps=values[1])
        self._angularMedians =  medians[0],medians[1]

    @property
    def angular_mean(self):
        return self._angularMeans

    @angular_mean.setter
    def angular_mean(self,values):
        means  = self._angular_resolution_perEbin(values[0], n_steps=values[1])
        self._angularMeans =  means[0],means[2]
            

    def binned_efficiency(self): 
        pass

    @classmethod
    def from_list_string_cuts(cls,inputfile,list_string_cuts):
        variables=[]
        cuts=[]
        relationals=[]
        
        for s in list_string_cuts:
            v,r,c= s.split(',')
            
            if r=='>':    np_r= np.greater
            elif r=='<':  np_r= np.less
            elif r=='==': np_r= np.equal
            elif r=='>=': np_r= np.greater_equal
            elif r=='<=': np_r= np.less_equal
            elif r=='!=': np_r= np.not_equal
            else:
                raise ValueError('Symbol {0} can not be converted to numpy relational operator'.format(r))
        
            if '.' in c: val=float(c)
            else: val=int(c)

            variables.append(v)
            cuts.append(val)
            relationals.append(np_r)

        analysis= cls(inputFileName=inputfile, 
                      relational=relationals, 
                      cut_value=cuts, 
                      var_to_cut=variables)
         
        return analysis


    def binned_effective_area(self,z_min,z_max,**kwargs):
        n_bins        = kwargs.get('n_bins',50)

        n_events      = np.sum(self.infile.root.mc.GeneratedInfo.cols.genvol_numberOfEvents[:])
        n_gen_events  = kwargs.get('n_gen_events',n_events)

        fit_name      = kwargs.get('fit_name','')
        if fit_name: 
            print 'Calculating effective area at reco level'
            cut = np.logical_and( self.bool_cut,
                                  self.infile.root.reco.__getattr__(recofitname).cols.is_in_event[:])  
        else:
            print 'Calculating effective area at trigger level (no cuts)'
            shape = len(self.mc_particle.cols.dir_z[:])
            cut   = np.ones(shape,dtype=np.bool)

        inverSecondsPerYear = 1./31536000.

        if z_min>=z_max: 
            raise ValueError(('z_min has to be lower than z_max'+ 
                              'You gave me z_min={0}, z_max={1}').format(z_min,z_max))
       
        solidAngle      = 2*np.pi*np.fabs(z_min-z_max)
      
        dirs_z          = self.mc_particle.cols.dir_z[:]
        energies        = self.mc_particle.cols.energy[:]

        gammaFactor     = (1.0 - self.spectrum) 
        vectorEffArea   = ( energies**(-self.spectrum) ) * self.w1[:]
        scalarEffArea   = ( gammaFactor/n_gen_events*inverSecondsPerYear ) * (1/solidAngle)
        weightedEffArea = scalarEffArea*vectorEffArea

        cutsInEffArea   = np.logical_and.reduce((cut, dirs_z>=z_min, dirs_z<=z_max))   

        weight = weightedEffArea[:][cutsInEffArea]
        energy = energies[:][cutsInEffArea]

        content,edges = np.histogram(energy, 
                                     weights= weight,
                                     bins=n_bins)

        #E_2^(1-gamma)-E_1^(1-gamma)
        widths     = edges[1:]**gammaFactor-edges[:-1]**gammaFactor

        errorPow2  = np.histogram(energy, 
                                  weights= weight**2,
                                  bins=n_bins) [0]

        
        binned_content = content/widths
        
        use_first_point = kwargs.get('use_first_point',False)

        if not use_first_point:
            binned_energy  = (edges[1:]+edges[0:-1])/2.
        else :
            binned_energy  = edges[0:-1]

        binned_error   = np.sqrt(errorPow2) / np.fabs(widths)

        return binned_energy, binned_content, binned_error
    

    

        
