% GET_ANIMATION_STATE is the playback state for an animation controller.
%   GET_ANIMATION_STATE(X) is the playback state for the animation
%   controller named X. Possible values are
%     'Stop' if the animation is not playing and is at the first frame,
%     'Play Forward' if the animation is playing from beginning to end,
%     'Play Backward' if the animation is playing from end to beginning,
%     'Pause Forward' if the animation is paused and was in 'Play Forward', and
%     'Pause Backward' if the animation is paused and was in 'Play Backward'.
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
%      >> get_animation_state()
%      Play Forward
%      >> set_animation_state('stop')
%      Stop
lasterr('This command must be executed from Opticks.')
