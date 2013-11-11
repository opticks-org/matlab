% SET_STRETCH_VALUES sets display parameters for an Opticks raster element.
%   SET_STRETCH_VALUES(X, Y, Z, W, V) sets the maximum stretch value to X,
%   and the minimum stretch value to Y for the layer named Z displayed in the
%   window V for channel W. Both X and Y should be double-precision
%   floating-point values.
%
%   By default Z = empty string, indicating the topmost raster layer.
%   By default W = 'Gray', indicating the gray channel. Other valid values are:
%      'Red'
%      'Green'
%      'Blue'
%   By default V = empty string, indicating the active window.
lasterr('This command must be executed from Opticks.')
