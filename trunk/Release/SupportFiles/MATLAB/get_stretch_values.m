% GET_STRETCH_VALUES is the display parameters for an Opticks raster element.
%   GET_STRETCH_VALUES(X, Y, Z) gets the maximum and minimum stretch values
%   for the layer named X displayed in the window Y for channel Z.
%
%   By default X = empty string, indicating the topmost raster layer.
%   By default Y = 'Gray', indicating the gray channel. Other valid values are:
%      'Red'
%      'Green'
%      'Blue'
%   By default Z = empty string, indicating the active window.
%
%   If no variable names are assigned, the output of this function will be
%   stored in variables in the MATLAB workspace called 'stretch_max' and
%   'stretch_min'.
lasterr('This command must be executed from Opticks.')
