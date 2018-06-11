from distutils.core import setup
setup(name='CHRspendfrom',
      version='1.0',
      description='Command-line utility for chratos "coin control"',
      author='Gavin Andresen',
      author_email='gavin@chratosfoundation.org',
      requires=['jsonrpc'],
      scripts=['spendfrom.py'],
      )
