
def main():

    f1 = open("emp_in.csv", "w+")
    for i in range(10000):
        f1.write("{},\"addr_{}\",{}\r\n".format((i+1), (i+1), ((i+1)%100)))
    f1.close()

    f2 = open("dept_in.csv", "w+")
    for i in range(100):
        f2.write("{},\"dept_name{}\"\r\n".format((i+1), (i+1)))
    f2.close()


if __name__ == "__main__":
    main()

