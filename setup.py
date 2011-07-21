# -*- coding: utf-8 -*-
from distutils.core import setup
from distutils.extension import Extension

rlfl = Extension('rlfl',
                 define_macros = [('DEBUG', '0')],
                 sources = [
                    'src/random.c',
                    'src/list_t.c',
                    'src/rlf.c',
                    'src/los.c',
                    'src/path_astar.c',
                    'src/path_basic.c',
                    'src/project.c',
                    'src/fov_circular_raycasting.c',
                    'src/fov_recursive_shadowcasting.c',
                    'src/fov_diamond_raycasting.c',
                    'src/fov_permissive.c',
                    'src/fov_restrictive.c',
                    'src/fov_digital.c',
                    'src/rlftopy.c'
                ]
)
setup(
    name='rlfl',
    version='1.1',
    author='Jóhann T. Maríusson',
    author_email='jtm@robot.is',
    keywords = ["game", "roguelike"],
    url='http://pypi.python.org/pypi/RLFL/',
    license='LICENSE.txt',
    description='Roguelike Function Library.',
    long_description=open('README').read(),
    ext_modules=[rlfl],
)