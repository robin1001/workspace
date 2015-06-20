#changes sums[i] = min(sums[i - j]) + 1, j in changes
changes = [1, 3, 5, 10]
sums = [0] * 23
for i in range(1, len(sums)):
    tmp = 11111111
    for x in changes:
        if i-x >= 0 and tmp > sums[i - x]:
            tmp = sums[i-x]
    sums[i] = tmp + 1

for x in sums:
    print x