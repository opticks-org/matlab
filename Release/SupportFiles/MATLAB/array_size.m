% ARRAY_SIZE Number of rows, columns, and bands in an Opticks raster element.
%   ARRAY_SIZE(X) is a 1-by-3 matrix M such that
%      M(1) is the number of rows in X,
%      M(2) is the number of columns in X, and
%      M(3) is the number of bands in X.
%
%   The default value is X = the primary raster element of the active window.
%
%   Example:
%      >> A = zeros(2, 3, 4);
%      >> array_to_opticks('A')
%      >> A_size = array_size()
%      A_size =
%         2 3 4
lasterr('This command must be executed from Opticks.')
