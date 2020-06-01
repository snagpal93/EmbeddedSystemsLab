#!/usr/bin/python3.4

import os, sys, csv

def main():
    if len(sys.argv)<2:
        print("Example: resultsToCsv \"path_to_resultsfolder\" \"path_to_csv\" benchmark[bench|act|benchi|acti]")
        return 1;
    if sys.argv[1]=="-h" or sys.argv[1]=="-help":
        print("Example: resultsToCsv \"path_to_resultsfolder\" \"path_to_csv\" benchmark[bench|act|benchi|acti]")
        return 1;
    if len(sys.argv)!=4:
        print("Example: resultsToCsv \"path_to_resultsfolder\" \"path_to_csv\" benchmark[bench|act|benchi|acti]")
        return 1;
    result_path = sys.argv[1]
    output_csv = sys.argv[2]
    if sys.argv[3] == "bench":
        benchmarklist = ["1","Execution time"]
    if sys.argv[3] == "benchi":
        benchmarklist = ["IDLE","1","Execution time"]
    if sys.argv[3] == "act":
        benchmarklist = ["3", "init", "VLD", "IQZZ", "IDCT", "CC", "Raster"]
    if sys.argv[3] == "acti":
        benchmarklist = ["IDLE", "3", "init", "VLD", "IQZZ", "IDCT", "CC", "Raster"]
    with open(output_csv, "w") as output:
        output.write("Core, Timestamp, Value, Image, actor\n")
    for subdir in os.walk(result_path):
        print("Subdir: ")
        print(subdir[0])
        mb1 = []
        mb2 = []
        mb3 = []
        mb4 = []
        try:
            with open(os.path.join(subdir[0], "response.txt"),encoding='utf-8', errors='ignore') as fp:
                line = fp.readline()
                while line:
                    words = line.split()
                    if len(words)>0:
                        if words[0] == "MB1":
                            mb1.append(words[0:6:2])
                            mb1[-1].append(str(os.path.basename(subdir[0])))
                            mb1[-1].append(benchmarklist[len(mb1)-1])
                        if words[0] == "MB2":
                            mb2.append(words[0:6:2])
                            mb2[-1].append(str(os.path.basename(subdir[0])))
                            mb2[-1].append(benchmarklist[len(mb2) - 1])
                        if words[0] == "MB3":
                            mb3.append(words[0:6:2])
                            mb3[-1].append(str(os.path.basename(subdir[0])))
                            mb3[-1].append(benchmarklist[len(mb3) - 1])
                        if words[0] == "MB4":
                            mb4.append(words[0:6:2])
                            mb4[-1].append(str(os.path.basename(subdir[0])))
                            mb4[-1].append(benchmarklist[len(mb4) - 1])
                        if words[0] == "Sending":
                            break
                    line = fp.readline()
        except IOError:
            print("No File found")
        # Assuming res is a list of lists
        with open(output_csv, "a") as output:
            writer = csv.writer(output, lineterminator='\n')
            writer.writerows(mb1)
            writer.writerows(mb2)
            writer.writerows(mb3)
            writer.writerows(mb4)


def faaltje(filepath):
    with open(filepath) as fp:
        line = fp.readline()
        cnt = 1
        while line:
            print("Line {}: {}".format(cnt, line.strip()))
            line = fp.readline()
            cnt += 1

if __name__ == "__main__":
    main()
