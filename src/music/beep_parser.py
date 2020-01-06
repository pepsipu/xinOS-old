import bashlex
beep_cmd = ""
parsed = bashlex.parse(beep_cmd)[0].parts[1:]
beeps = []
current_structure = {
    "freq": 0,
    "len": 100,
    "delay": 0
}
next_is_val = (False, "")
for x in parsed:
    word = x.word
    if word == "-n":
        beeps.append(current_structure)
        current_structure = {
            "freq": 0,
            "len": 100,
            "delay": 0
        }
    elif word == "-f":
        next_is_val = (True, "freq")
    elif word == "-l":
        next_is_val = (True, "len")
    elif word == "-D":
        next_is_val = (True, "delay")
    else:
        if not next_is_val[0]:
            print("invalid syntax")
            exit(1)
        current_structure[next_is_val[1]] = int(word)
beeps.append(current_structure)
for x in beeps:
    print("{.freq = " + str(x["freq"]) + ", .len = " + str(x["len"]) + ", .delay = " + str(x["delay"]) + "},")