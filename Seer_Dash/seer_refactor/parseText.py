
def readFile(filename):
    f = open(filename, "r")
    return f


def parseText(text, seperator, variables):
    outputDict = {}
    index = 0
    for line in text:
        if line.startswith(seperator, 0, len(seperator)):
            output_line = text.next()           # find the first "*_*_*"

            if output_line[-1] == ',':          # if there is a ',' at the end
                output_line = output_line[:-1]
                outputDict[ variables[index] ] = output_line    #store line in dictionary

            output_line = text.next()           # skip the second the first "*_*_*"

            continue

    return outputDict



if __name__ == "__main__":
    f = readFile()
    parseText(f, "*_*_*")
