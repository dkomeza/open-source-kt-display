b2 = 245
b4 = 169

speed = []
wheel = []
binary = list(bin((b2 << 8 | b4))[2:])

speed.append(binary[10])
speed.append(binary[0])
speed.append(binary[1])
speed.append(binary[2])
speed.append(binary[3])
speed.append(binary[4])
speed = "".join(speed)

print(int(speed, 2) + 10)

wheel.append(binary[5])
wheel.append(binary[6])
wheel.append(binary[7])
wheel.append(binary[8])
wheel.append(binary[9])

wheel = "".join(wheel)

print(int(wheel, 2))

# for i, bit in enumerate(binary):

#     print(i,":",binary[i])