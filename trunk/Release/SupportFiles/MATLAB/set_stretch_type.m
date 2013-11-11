% SET_STRETCH_TYPE sets the stretch type for an Opticks raster element.
%   SET_STRETCH_TYPE(X, Y, Z, W) sets the stretch type to X for the layer
%   named Y displayed in the window W for channel Z.
%
%   Valid values for X are:
%      'Linear'
%      'Logarithmic'
%      'Exponential'
%      'Equalization'
%   By default Y = empty string, indicating the topmost raster layer.
%   By default Z = 'Grayscale', indicating the gray channel. Other valid values are:
%      'Rgb'
%   By default W = empty string, indicating the active window.
lasterr('This command must be executed from Opticks.')
