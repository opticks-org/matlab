% DISABLE_GPU turns off dynamic Cg filtering for the specified raster layer.
%   DISABLE_GPU(X, Y) causes the raster layer named X in the window Y to not be
%   rendered using the standard graphics processing unit (GPU).
%
%   The default value of X is an empty string indicating the top raster layer.
%   The default value of Y is an empty string indicating the active window.
%
%   See also DISABLE_FILTER.
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
