% ARRAY_TO_MATLAB Copies data from an Opticks raster element to MATLAB.
%   ARRAY_TO_MATLAB() copies the entire primary raster element of the active
%   window into the MATLAB workspace.
%
%   ARRAY_TO_MATLAB(C0, C1, R0, R1, B0, B1, X) copies a subset of X where
%      C0 is the first column of data to copy.
%      C1 is the last column of data to copy.
%      R0 is the first row of data to copy.
%      R1 is the last row of data to copy.
%      B0 is the first band of data to copy.
%      B1 is the last band of data to copy.
%      X is the primary raster element of the active window.
%
%      C1, R1, and/or B1 can be assigned the sentinel value of 0 to copy all
%      available data if the actual size is not known.
%
%   If no variable name is assigned, the output of this function will be stored
%   in a variable in the MATLAB workspace called 'raster'.
%
%   The size of the copy will be M-by-N for single band data.
%   The size of the copy will be M-by-N-by-B for multi-band data.
%
%   Data type will be preserved during the copy. See CLASS.
%
%   This function supports data up to 2 GB in size.
%   To determine the size of the data, use the the following formula:
%      total_size = row_count * column_count * band_count * data_size_in_bytes
%
%   Example:
%      >> B = zeros(2, 3, 4);
%      >> B(:,:,1) = [1, 2 , 3; 4 , 5, 6];
%      >> B(:,:,2) = [7, 8 , 9; 10, 11, 12];
%      >> B(:,:,3) = [13, 14, 15; 16, 17, 18];
%      >> B(:,:,4) = [19, 20, 21; 22, 23, 24];
%      >> array_to_opticks('B', '', 1, 1, 'bip')
%      >> array_to_matlab()
%      >> raster
%      raster(:,:,1) =
%           1     2     3
%           4     5     6
%      raster(:,:,2) =
%           7     8     9
%          10    11    12
%      raster(:,:,3) =
%          13    14    15
%          16    17    18
%      raster(:,:,4) =
%          19    20    21
%          22    23    24
lasterr('This command must be executed from Opticks.')
