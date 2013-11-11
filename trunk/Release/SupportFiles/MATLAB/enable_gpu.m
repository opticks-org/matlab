% ENABLE_GPU turns on dynamic Cg filtering for the specified raster layer.
%   ENABLE_GPU(X, Y) causes the raster layer named X in the window Y to be
%   rendered using the graphics processing unit (GPU) as well as any active
%   Cg filters.
%
%   The default value of X is an empty string indicating the top raster layer.
%   The default value of Y is an empty string indicating the active window.
%
%   See also ENABLE_FILTER.
%
%   Example:
%      Sample Data: http://opticks.org/confluence/display/opticks/Sample+Data
%      In Opticks, load a raster element with multiple bands.
%      >> get_filter_names()
%      ans =
%      ByPass       
%      EdgeDetection
%      >> enable_gpu()
%      >> enable_filter('ByPass')
%      >> enable_filter('EdgeDetection')
%      >> disable_filter('ByPass')
%      >> disable_gpu()
lasterr('This command must be executed from Opticks.')
