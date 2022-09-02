
import fast
import os
import SimpleITK as sitk
import sys
import getopt
import numpy as np


def segmentAirwaysFAST(filenameInput, filenameOutputAirways):
    importer = fast.ImageFileImporter.create(filenameInput)
    image = importer.runAndGetOutputData()
    print("An image was imported with size: ", image.getWidth(), image.getHeight())

    airways_filter = fast.AirwaySegmentation.create().connect(importer)
    segmentation_airways = airways_filter.runAndGetOutputData()

    if np.asarray(segmentation_airways).size < 1000:
        print("Error: Could not segment airways.")
        return
    
    exporter_airways = fast.MetaImageExporter.create(filenameOutputAirways).connect(segmentation_airways)
    exporter_airways.run()


InputVolume = ''
OutputLabel = ''
def read_arguments(arguments):
    global InputVolume
    global OutputLabel

    arg_list = arguments.strip().split(',')
    #print('arg_list: ' + arg_list)
    print('---')
    print(arg_list)
    print('---')
    pairs = [[x.split(' ')[0], x.split(' ')[1]] for x in arg_list]
    param_dict = dict(pairs)
    InputVolume = param_dict['InputVolume']
    OutputLabel = param_dict['OutputLabel']

    if InputVolume == '' or OutputLabel == '':
        print('Input volume or output volume is empty, cannot proceed!')
        sys.exit()

    print(InputVolume)
    print(OutputLabel)


def main(argv):
    os.environ["CUDA_VISIBLE_DEVICES"] = "0,1"

    arguments = ''
    task = ''
    try:
        opts, args = getopt.getopt(argv, "ht:a:", ["Task=", "Arguments="])
    except getopt.GetoptError:
        print('usage: main.py -Task <TaskName> --Arguments <ArgumentsList>')
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            print('main.py -Task <TaskName> --Arguments <ArgumentsList>')
            sys.exit()
        elif opt in ("-t", "--Task"):
            task = arg
        elif opt in ("-a", "--Arguments"):
            arguments = arg

    # Constants and input variables
    n_argin_expected = 2  # Expect input and output volume paths
    if len(sys.argv) > n_argin_expected:  # command string is sys.argv[0]
        input_image_path = sys.argv[1]  # First argument should always be input volume
        print(input_image_path)
        output_image_path = sys.argv[2]  # Second argument should always be destination volume file
        print(output_image_path)
    else:
        print('Too few arguments, script aborted.')
        exit(1)  # TODO: Find proper exit code
    # Get input arguments
    print('Arguments: ' + arguments)
    #read_arguments(arguments)

    segmentAirwaysFAST(input_image_path, output_image_path)


if __name__ == "__main__":
    main(sys.argv[1:])


