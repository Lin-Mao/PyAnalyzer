all:
	python setup.py bdist
	pip install .

clean:
	rm -rf build dist pyspy.egg-info
	pip uninstall pyspy -y
