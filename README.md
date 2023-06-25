# PyAnalyzer

A Python code analyzer (under development).

## How to use?

### Installation

```shell
git clone --recursive https://github.com/Lin-Mao/PyAnalyzer.git
python setup.py bdist
pip install .
```

### Code example

`tests/test.py`
```python
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
```

output:

```
main is called
a is called
b is called
c is called

f-0  /home/lm/PyAnalyzer/tests/test.py:3  def c():6
f-1  /home/lm/PyAnalyzer/tests/test.py:8  def b():10
f-2  /home/lm/PyAnalyzer/tests/test.py:12  def a():14
f-3  /home/lm/PyAnalyzer/tests/test.py:16  def main():18
f-4  /home/lm/PyAnalyzer/tests/test.py:1  def <module>():21
```


