# -*- coding: utf-8 -*-
# File created by sphinx-quickstart, edited as convenient. 

import os, sys
sys.path.insert(0, os.path.abspath('.'))

extensions = ['sphinx.ext.autodoc',
              'sphinxcontrib.plantuml',
              'sphinxarg.ext',
              'sphinx.ext.autosectionlabel']

plantuml = 'java -jar '+os.path.expandvars('$HOME/')+'plantuml.jar'

source_suffix = '.rst'

master_doc = 'index'

project = u'KM3NeT Online Reconstruction'
copyright = u'2019, Giuliano Maggi Olmedo'
author = u'Giuliano Maggi Olmedo'

version = u'1'
release = u'1'

language = None

# The name of the Pygments (syntax highlighting) style to use.
pygments_style = 'sphinx'

# If true, `todo` and `todoList` produce output, else they produce nothing.
todo_include_todos = False

html_theme = 'classic'

htmlhelp_basename = 'KM3NeTOnlineReconstructiondoc'


# -- Options for LaTeX output ---------------------------------------------

latex_elements = {
    # The paper size ('letterpaper' or 'a4paper').
    #
    # 'papersize': 'letterpaper',

    # The font size ('10pt', '11pt' or '12pt').
    #
    # 'pointsize': '10pt',

    # Additional stuff for the LaTeX preamble.
    #
    # 'preamble': '',

    # Latex figure (float) alignment
    #
    # 'figure_align': 'htbp',
}

# Grouping the document tree into LaTeX files. List of tuples
# (source start file, target name, title,
#  author, documentclass [howto, manual, or own class]).
latex_documents = [
    (master_doc, 'KM3NeTOnlineReconstruction.tex', u'KM3NeT Online Reconstruction Documentation',
     u'Giuliano Maggi Olmedo', 'manual'),
]


# -- Options for manual page output ---------------------------------------

# One entry per manual page. List of tuples
# (source start file, name, description, authors, manual section).
man_pages = [
    (master_doc, 'km3netonlinereconstruction', u'KM3NeT Online Reconstruction Documentation',
     [author], 1)
]


# -- Options for Texinfo output -------------------------------------------

# Grouping the document tree into Texinfo files. List of tuples
# (source start file, target name, title, author,
#  dir menu entry, description, category)
texinfo_documents = [
    (master_doc, 'KM3NeTOnlineReconstruction', u'KM3NeT Online Reconstruction Documentation',
     author, 'KM3NeTOnlineReconstruction', 'One line description of project.',
     'Miscellaneous'),
]



