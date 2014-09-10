import argparse
import errno
import glob
import subprocess
import os
from string import Template

parser = argparse.ArgumentParser(description='Save SVG renderer outputs as a report.')
parser.add_argument('files', nargs='+', 
                   help='glob expressions (patterns) for SVG files')
parser.add_argument('--executable', dest='executable', required=True,
                   help='path to demo app executable')
parser.add_argument('--report_dir', dest='report_dir', default='report',
                   help='path to report output folder')

args = parser.parse_args()

template_dir = os.path.join(os.path.dirname(os.path.realpath(__file__)), "report_templates")

with open(os.path.join(template_dir, "record.tmpl"), "r") as tmpl_file:
  record_template=Template(tmpl_file.read())

try:
 os.makedirs(args.report_dir)
except OSError as e:
  if e.errno == errno.EEXIST:
      pass
  else:
      raise
  
report_records = ''
for glob_arg in args.files:
  for file in glob.iglob(glob_arg):
    out_image_name = os.path.join(args.report_dir, os.path.basename(file) + '.png')
    if subprocess.call([args.executable, file, out_image_name]) == 0:
      report_records += record_template.substitute(header=file, raster_file=out_image_name, svg_file=file)

with open(os.path.join(template_dir, "report.tmpl"), "r") as tmpl_file:
  with open("report.html", "w") as report_file:
    report_file.write(Template(tmpl_file.read()).substitute(records = report_records))
      
