def add():
    global t
    global fill
    thickness = max(1, round((64-len(t))/6.4))
    t.extend([fill]*thickness)
    fill = '.' if fill == 'X' else 'X'

for i in range(10):
    t = ['X']*(10-i)

    fill = '.'

    while len(t) < 64:
        add()

    arr = []

    for i in range(64):
        arr.append(i if t[63-i] == 'X' else i+64)

    print('{' + ','.join([str(val) for val in arr]) + '},')

for i in range(10):
    t = ['.']*(10-i)

    fill = 'X'

    while len(t) < 64:
        add()

    arr = []

    for i in range(64):
        arr.append(i if t[63-i] == 'X' else i+64)

    print('{' + ','.join([str(val) for val in arr]) + '},')
