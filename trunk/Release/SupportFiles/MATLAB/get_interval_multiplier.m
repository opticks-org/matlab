% GET_INTERVAL_MULTIPLIER is the playback speed for an animation controller.
%   GET_INTERVAL_MULTIPLIER(X) is the desired playback speed for the
%   animation controller named X.
%   The actual playback speed may differ from this value.
%
%   See also ENABLE_CAN_DROP_FRAMES, DISABLE_CAN_DROP_FRAMES.
%
%   The default value is X = the active animation controller.
%
%   Example:
%      >> B = zeros(2, 3, 4);
%      >> B(:,:,1) = [1, 2 , 3; 4 , 5, 6];
%      >> B(:,:,2) = [7, 8 , 9; 10, 11, 12];
%      >> B(:,:,3) = [13, 14, 15; 16, 17, 18];
%      >> B(:,:,4) = [19, 20, 21; 22, 23, 24];
%      >> array_to_opticks('B')
%      >> create_animation()
%      >> set_interval_multiplier(3)
%      >> set_animation_cycle('repeat')
%      >> set_animation_state('play_forward')
%      >> set_animation_state('stop')
%      >> get_interval_multiplier()
%      ans =
%         3
lasterr('This command must be executed from Opticks.')
