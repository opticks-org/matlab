% GET_FILTER_NAMES is the list of available Cg filters.
%   GET_FILTER_NAMES(X, Y) is the list of available Cg filters for the raster
%   layer named X in the window Y to be which can be used.
%
%   The default value of X is an empty string indicating the top raster layer.
%   The default value of Y is an empty string indicating the active window.
%
%   See also ENABLE_FILTER, DISABLE_FILTER, ENABLE_GPU, DISABLE_GPU.
%
%   Example:
%      Sample Data: http://opticks.org/confluence/display/opticks/Sample+Data
%      In Opticks, load a raster element with multiple bands.
%      >> get_filter_names()
%      ByPass, EdgeDetection
%      >> enable_gpu()
%      >> enable_filter('ByPass')
%      >> enable_filter('EdgeDetection')
%      >> disable_filter('ByPass')
%      >> disable_gpu()
lasterr('This command must be executed from Opticks.')
