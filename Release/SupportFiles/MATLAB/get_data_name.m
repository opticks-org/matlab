% GET_DATA_NAME is the name of a data element being displayed in a window.
%   GET_DATA_NAME(X, Y, Z) is the name of the data element of the layer named
%   X which is being displayed in the layer Y. If the value of Z is 't',
%   'true', 1, or any other similar value indicating true, then only raster
%   elements will be considered by this search. Otherwise, any type of element
%   with matching parameters may be returned.
%
%   The default is X = empty string, indicating the topmost layer.
%   The default is Y = empty string, indicating the active window.
%   The default is Z = '1', indicating to only search for raster elements
lasterr('This command must be executed from Opticks.')
