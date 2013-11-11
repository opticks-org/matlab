% GET_DATA_ELEMENT_NAMES is the list of child elements of the specified window.
%   GET_DATA_ELEMENT_NAMES(X, Y) shows the child elements of type Y of the
%   primary raster element displayed by the spatial data window named X.
%
%   The default values are
%      X = the name of the active spatial data window
%      Y = empty string to indicate any type but other valid values include:
%        'AoiElement'
%        'GcpList'
%        'AnnotationElement'
%        and many more.
%
%   The value of X is case sensitive.
%   The value of Y is case sensitive.
lasterr('This command must be executed from Opticks.')
