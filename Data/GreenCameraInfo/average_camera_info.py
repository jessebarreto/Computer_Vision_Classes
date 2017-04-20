
import os
import numpy


def calculate_cam_info_mean_std(filepath):
	# files = [f for f in os.listdir(filepath) if os.path.isfile(join(filepath, f))]
	files = os.listdir(filepath)
	files.remove(os.path.basename(__file__))
	if "Distortion_mean.xml" in files:
		files.remove("Distortion_mean.xml")
	if "Intrinsics_mean.xml" in files:
		files.remove("Intrinsics_mean.xml")

	distortions_files = []
	intrisics_files = []
	for f in files:
		if "Distortion" in f:
			distortions_files.append(f)
		elif "Intrinsics" in f:
			intrisics_files.append(f)

	distortions = []
	for f in distortions_files:
		file = open(f, "r")
		for line in file:
			if "</data></Distortion>" in line:
				line = line.replace("</data></Distortion>","")
				line = line.split();
				d = []
				for i in line:
					d.append(float(i))
				distortions.append(d)
		file.close()

	k1 = []
	k2 = []
	p1 = []
	p2 = []
	for i in range(0, len(distortions)):
		k1.append(distortions[i][0])
		k2.append(distortions[i][1])
		p1.append(distortions[i][2])
		p2.append(distortions[i][3])

	dist_mean = [numpy.mean(k1), numpy.mean(k2), numpy.mean(p1), numpy.mean(p2)]
	dist_std = [numpy.std(k1), numpy.std(k2), numpy.std(p1), numpy.std(p2)]

	intrinsics = []
	for f in intrisics_files:
		file = open(f, "r")
		lines = file.read().split("\n")
		intr = lines[7] + lines[8]
		intr = intr.replace("</data></Intrinsics>","")
		intr = intr.split()
		i = []
		for l in intr:
			i.append(float(l))
		intrinsics.append(i)
		file.close()


	fx = []
	fy = []
	cx = []
	cy = []
	for i in range(0, len(intrinsics)):
		fx.append(intrinsics[i][0])
		fy.append(intrinsics[i][2])
		cx.append(intrinsics[i][4])
		cy.append(intrinsics[i][5])

	intr_mean = [numpy.mean(fx), numpy.mean(fy), numpy.mean(cx), numpy.mean(cy)]
	intr_std = [numpy.std(fx), numpy.std(fy), numpy.std(cx), numpy.std(cy)]


	return dist_mean, dist_std, intr_mean, intr_std

def save_distortion_info(dist_mean, dist_std):
	file = open("Distortion_mean.xml", "w+")
	file.write("<?xml version=\"1.0\"?>\n")
	file.write("<opencv_storage>\n")
	file.write("<Distortion type_id=\"opencv-matrix\">\n")
	file.write("  <rows>4</rows>\n")
	file.write("  <cols>1</cols>\n")
	file.write("  <dt>f</dt>\n")
	file.write("  <data>\n")
	file.write("    ")
	for i in range(0,4):
		file.write("%.8e" % dist_mean[i])
		if i < 3:
			file.write(" ")
	file.write("</data></Distortion>\n")
	file.write("</opencv_storage>")
	file.close()
	print("Distortions Average")
	print(dist_mean)
	print("Distortions Standard Deviation")
	print(dist_std)

def save_intrisincs_info(intr_mean, intr_std):
	file = open("Intrinsics_mean.xml", "w+")
	file.write("<?xml version=\"1.0\"?>\n")
	file.write("<opencv_storage>\n")
	file.write("<Intrinsics type_id=\"opencv-matrix\">\n")
	file.write("  <rows>3</rows>\n")
	file.write("  <cols>3</cols>\n")
	file.write("  <dt>f</dt>\n")
	file.write("  <data>\n")
	file.write("    ")
	file.write("%.8e" % intr_mean[0])
	file.write(" 0. ")
	file.write("%.8e" % intr_mean[1])
	file.write(" 0. ")
	file.write("%.8e" % intr_mean[2])
	file.write(" ")
	file.write("%.8e" % intr_mean[3])
	file.write(" 0.\n")
	file.write("    0. 1.</data></Intrinsics>\n")
	file.write("</opencv_storage>\n")
	file.close()
	print("Distortions Average")
	print(intr_mean)
	print("Distortions Standard Deviation")
	print(intr_std)



	# file.write("    -3.45301390e+00 -7.27336578e+01 -1.75841048e-01 9.58619267e-02</data></Distortion>\n")
	# file.write("</opencv_storage>\n")


def main():
	[dist_mean, dist_std, intr_mean, intr_std] = calculate_cam_info_mean_std(".")
	save_distortion_info(dist_mean, dist_std)
	save_intrisincs_info(intr_mean, intr_std)

#this calls the 'main' function when this script is executed
if __name__ == '__main__':
    main()