% SET_STRETCH_UNITS sets the stretch units for an Opticks raster element.
%   SET_STRETCH_UNITS(X, Y, Z, W) sets the stretch units to X for the layer
%   named Y displayed in the window W for channel Z.
%
%   Valid values for X are:
%      'Raw'
%      'Raw Value'
%      'Percentage'
%      'Percentile'
%      'StdDev'
%   By default Y = empty string, indicating the topmost raster layer.
%   By default Z = 'Gray', indicating the gray channel. Other valid values are:
%      'Red'
%      'Green'
%      'Blue'
%   By default W = empty string, indicating the active window.
lasterr('This command must be executed from Opticks.')
