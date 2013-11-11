% GET_STRETCH_TYPE is the stretch type for an Opticks raster element.
%   GET_STRETCH_TYPE(X, Y, Z) is the stretch type for the layer
%   named X displayed in the window Z for display mode Y.
%
%   Valid values for X are:
%      'Linear'
%      'Logarithmic'
%      'Exponential'
%      'Equalization'
%   By default X = empty string, indicating the topmost raster layer.
%   By default Y = 'Grayscale', indicating the gray channel. Other valid values are:
%      'Rgb'
%   By default Z = empty string, indicating the active window.
lasterr('This command must be executed from Opticks.')
