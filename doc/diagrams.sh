#!/bin/bash

set -e

tmp_dir=../build/images
dot_dirs=(images ../firmware/pic/doc/images)
tex_dirs=(../firmware/pic/doc/images)

for dir in ${dot_dirs[@]}; do
	for f in ${dir}/*.dot; do
		new_f="${f%.dot}.png"
		echo "${f} -> ${new_f}"
		dot -Tpng -o "${new_f}" "${f}"
	done
done

mkdir -p ${tmp_dir}
for dir in ${tex_dirs[@]}; do
	for f in ${dir}/*.tex; do
		dvi_f="${tmp_dir}/$(basename $f)"
		dvi_f="${dvi_f%.tex}.dvi"
		png_f="${f%.tex}.png"
		echo "${f} -> ${dvi_f} -> ${png_f}"
		latex -output-directory=${tmp_dir} ${f}
		dvipng -T tight --bg Transparent -z 9 -o ${png_f} ${dvi_f}
	done
done
