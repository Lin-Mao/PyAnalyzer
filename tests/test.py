import pyanalyzer

def c():
    print("c is called")
    print()
    print(pyanalyzer.get_frames())
    
def b():
    print("b is called")
    c()

def a():
    print("a is called")
    b()

def main():
    print("main is called")
    a()

if __name__ == "__main__":
    main()