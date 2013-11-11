% GET_STRETCH_UNITS is the stretch units for an Opticks raster element.
%   GET_STRETCH_UNITS(X, Y, Z) is the stretch units for the layer
%   named X displayed in the window Z for channel Y.
%
%   Valid return values are:
%      'Raw'
%      'Raw Value'
%      'Percentage'
%      'Percentile'
%      'StdDev'
%   By default X = empty string, indicating the topmost raster layer.
%   By default Y = 'Gray', indicating the gray channel. Other valid values are:
%      'Red'
%      'Green'
%      'Blue'
%   By default Z = empty string, indicating the active window.
lasterr('This command must be executed from Opticks.')
