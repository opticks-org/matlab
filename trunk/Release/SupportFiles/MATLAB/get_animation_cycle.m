% GET_ANIMATION_CYCLE is the animation behavior for an animation controller.
%   GET_ANIMATION_CYCLE(X) is the animation behavior for the animation
%   controller named X. Possible values are
%     'Play_Once' to stop playing the animation once it has finished,
%     'Repeat' to loop playback until stopped by the user, and
%     'Bounce' to toggle playback direction forward and backward.
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
%      >> get_animation_cycle()
%      Repeat
lasterr('This command must be executed from Opticks.')
