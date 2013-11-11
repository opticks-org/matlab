% SET_METADATA sets a value in an Opticks element or wizard.
%   SET_METADATA(X, Y, B0, B1, W, Z) does the following:
%      Queries the MATLAB workspace for variable Y.
%      If B0 is 't', 'true', 1, or a similar value indicating true:
%         Y will be forced to type bool (logical 0 or 1).
%      Else if B1 is 't', 'true', 1, or a similar value indicating true:
%         Y will be forced to type Filename (string).
%      If W is an empty string:
%         The metadata attribute X from the element Z is set to Y.
%      If W is not an empty string:
%         Z is ignored.
%         The wizard file W is loaded.
%         The wizard object value Y from wizard W is set to Y.
%
%   See also GET_METADATA, RELOAD_WIZARD.
lasterr('This command must be executed from Opticks.')
