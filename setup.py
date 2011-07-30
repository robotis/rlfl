# -*- coding: utf-8 -*-
from distutils.core import setup
from distutils.extension import Extension

rlfl = Extension('rlfl',
                 define_macros = [
                    ('DEBUG', '0'),
                    ('RLFL_MAX_MAPS', 16),
                    ('RLFL_MAX_PATHS', 16),
                    ('RLFL_MAX_PROJECTS', 16),
                    ('RLFL_MAX_RANGE', 60),
                    ('RLFL_MAX_RADIUS', 60),
                    ('RLFL_MAX_WIDTH', 5000),
                    ('RLFL_MAX_HEIGHT', 5000),
                 ],
                 sources = [
                    'src/random.c',
                    'src/list_t.c',
                    'src/rlfl.c',
                    'src/los.c',
                    'src/dijkstra.c',
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
    version='2.2',
    author='Jóhann T. Maríusson',
    author_email='jtm@robot.is',
    keywords = ["game", "roguelike"],
    url='http://pypi.python.org/pypi/RLFL/',
    license='LICENSE.txt',
    description='Roguelike Function Library.',
    long_description=open('README').read(),
    ext_modules=[rlfl],
)