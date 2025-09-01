all:
	python setup.py bdist
	pip install .

clean:
	rm -rf build dist pyanalyzer.egg-info
	pip uninstall pyanalyzer -y
