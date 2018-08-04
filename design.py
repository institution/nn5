import operator
import functools

def mul(xs):
	return functools.reduce(operator.mul, xs, 1);

# Game of Go Neural Network Complexity


float_size = 4


#board_dim = (3,3)
#board_dim = (5,5)
#board_dim = (7,7)
board_dim = (9,9)
#board_dim = (11,11)
#board_dim = (13,13)
MAX_BOARD_DIM = 13



"""
RULESET:
komi: +5.5
board: 9x9
handicap: 0  (random handicap)
"""

"""
packed repr:
* 
	header
	moves


need sfg packer

"""

"""
unpacked repr:

64 board state
	26 black stones (2b*13)
	26 white stones
	12 
		1 player: BlackMove, WhiteMove, BlackWonS, WhiteWonS, BlackWonR, WhiteWonR
		1 prev_move
		2 score
		1 future winner
		1 future move				
		2 future score
		




"""
gostate_size = 80

print("max_1mil_game_db[MB]", gostate_size * 81)


"""
input
plane 0: on-the-move player stones + pad
plane 1: other player stones + pad
plane 2: border (1 if outside board, dim = board_dim + (2,2))
"""
input_dim = (board_dim[0]+2, board_dim[1]+2, 3)
input_size = mul(input_dim) * float_size
print("input_dim", input_dim)
print("input_size", input_size)
print()

nfilter = 64

convf_filter_dim = (3,3,input_dim[2])
convf_size = mul(convf_filter_dim) * nfilter * float_size
convf_op = mul(convf_filter_dim) * mul(board_dim) * nfilter
print("convf_filter_dim", convf_filter_dim)
print("convf_size", convf_size)
print("convf_op", convf_op)
print()

tower_dim = (board_dim[0]+2, board_dim[1]+2, nfilter)
tower_size = mul(tower_dim) * float_size
print("tower_dim", tower_dim)
print("tower_size", tower_size)
print()

convn_filter_dim = (3,3,nfilter)
convn_size = mul(convn_filter_dim) * float_size * nfilter
convn_op = mul(convn_filter_dim) * mul(board_dim) * nfilter
print("convn_filter_dim", convn_filter_dim)
print("convn_size", convn_size)
print("convn_op", convn_op)
print()


nn_op = convf_op + convn_op*10
print("nn_w_size",             convf_size +            convn_size * 10)
print("nn_v_size", input_size +           tower_size +                 + tower_size*10)
print("nn_op", nn_op)




flops = 2258 * 10**9
#flops = (10000**2)/2.0
print("gpu_flop", flops)
eval_per_game = mul(board_dim) * mul(board_dim)/2.0
print("eval_per_game", eval_per_game)
op_per_game = eval_per_game * nn_op
print("op_per_game", op_per_game)
time_per_game_s = op_per_game / flops
print("time_per_game[s,h]", time_per_game_s, time_per_game_s/3600.0)
s_per_train = time_per_game_s * 1000000;
print("time_per_game x 10^6 [h,d]=", s_per_train / 3600.0, s_per_train / (3600*24.0))

	
#fc: 9x9x128 -> 1024
#	1024
#fc: 1024 -> 1
#	1
