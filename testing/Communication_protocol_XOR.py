packet_down = [65,16,48,0,139,0,164,2,13,0,0,0]

ui8_crc_down = 0
for ui8_i in range(len(packet_down)):
    if ui8_i != 6 and ui8_i != 0:
        ui8_crc_down ^= packet_down[ui8_i]
print(ui8_crc_down, packet_down[6])

packet_up = [12,0,149,160,41,102,18,74,4,20,0,50,14]
ui8_crc_up = 0
for ui8_i in range(len(packet_up)):
    if ui8_i != 5:
        ui8_crc_up ^= packet_up[ui8_i]
ui8_crc_up ^= 2
print(ui8_crc_up, packet_up[5])