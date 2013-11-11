% SET_ANIMATION_STATE sets the playback state for an animation controller.
%   SET_ANIMATION_STATE(X, Y) sets the playback state for the animation
%   controller named Y to the value of X. Possible values of X are
%     'Stop' if the animation should stop playing,
%     'Play_Forward' if the animation should play from beginning to end,
%     'Play_Backward' if the animation should play from end to beginning,
%     'Pause_Forward' if the animation should pause (was in 'Play Forward'), and
%     'Pause_Backward' if the animation should pause (was in 'Play Backward').
%
%   The default value is Y = the active animation controller.
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
lasterr('This command must be executed from Opticks.')
