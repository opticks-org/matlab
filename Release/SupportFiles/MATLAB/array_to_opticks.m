% ARRAY_TO_OPTICKS Copies data from MATLAB to an Opticks raster element.
%   ARRAY_TO_OPTICKS('M') copies the variable M from the MATLAB workspace to
%      a new Opticks raster element X such that
%         X has no parent.
%         X will be displayed as the primary raster element in a new view.
%         X will have band sequential (BSQ) interleave.
%         X will not have units defined (Digital Number).
%
%   ARRAY_TO_OPTICKS('M', 'P', D, W, 'I', 'U') copies the variable M from the
%      MATLAB workspace to a new Opticks raster element X such that
%        X will have parent P if P exists.
%        X will be displayed if D indicates 't', 'true', 1, or a similar value.
%        X will be displayed as the primary raster element in a new view if W
%            indicates 't', 'true', 1, or a similar value.
%        X will have interleave specified by I ('BIP', 'BIL', or 'BSQ').
%        X will have units specified by 'U' if U is not an empty string.
%
%   The size of the copy will always match the output from SIZE(M).
%   If subsetting is required, it must be done in MATLAB before calling ARRAY_TO_OPTICKS.
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
lasterr('This command must be executed from Opticks.')
