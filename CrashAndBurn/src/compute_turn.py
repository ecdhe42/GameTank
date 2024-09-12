import math

x = []
w = []
for i in range(64):
    width = 4 + i*(120-4)/64
    nb = round(width)
    w.append(nb)
    x.append(64 - round(nb/2))

#print(",".join([str(val) for val in x]))
#print(",".join([str(val) for val in w]))

shifts = [0, 0, 0, 0, x, 0, 0, 0, 0]

def get_shift(idx, factor):
    global shifts

    res_l = []
    res_r = []
    for line in range(64):
        res_l.append(str(x[63-line] - round(math.exp(line*factor))-1))
        res_r.append(str(x[63-line] + round(math.exp(line*factor))-1))
    res_l.reverse()
    res_r.reverse()
    shifts[idx] = res_l
    shifts[8-idx] = res_r

get_shift(0, 0.064)
get_shift(1, 0.060)
get_shift(2, 0.054)
get_shift(3, 0.045)
shifts[4] = [str(val) for val in x]

for shift in shifts:
    print(  "{"+",".join(shift) + "},")
