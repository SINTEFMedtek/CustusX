import setuptools

with open("README.md", "r") as fh:
    long_description = fh.read()

setuptools.setup(
     name='livermask',  
     version='0.1',
     author="Andre Pedersen",
     author_email="andre.pedersen@sintef.no",
     description="A package for automatic segmentation of liver from CT data",
     long_description=long_description,
     long_description_content_type="text/markdown",
     url="https://github.com/andreped/livermask",
     packages=setuptools.find_packages(),
     entry_points={
        'console_scripts': [
            'livermask = livermask.livermask:main'
        ]
     },
     install_requires=[
        'tensorflow==1.13.1',
        'numpy',
        'scipy',
        'tqdm',
        'nibabel',
        'h5py',
        'gdown',
        'scikit-image'
    ],
     classifiers=[
         "Programming Language :: Python :: 3",
         "License :: OSI Approved :: MIT License",
         "Operating System :: OS Independent",
     ],
     python_requires='>=3.6',
 )
