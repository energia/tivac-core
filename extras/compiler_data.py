# ==========================================================
# this file contains the data sturcture for the compiler
# Note: this is tools specific and needs to be adjusted
# ==========================================================

import json
from collections import OrderedDict

def getCompilerToolName(args):
	return "arm-none-eabi-gcc"

def get_platform(args, my_url):
	tool = OrderedDict([
		('name', "Energia TivaC boards"),
		('architecture', args.arch),
		('version', args.version),
		('category', "Energia"),
		('url', my_url + args.arch + "-" + args.version + ".tar.bz2"),
		('archiveFileName', args.arch + "-" + args.version + ".tar.bz2"),
		('checksum', "0"),
		('size', ""),
		("boards", [
			{"name": "EK-TM4C123GXL"},
			{"name": "EK-TM4C1294XL"},
			{"name": "EK-LM4F120XL"} 
		]),
		("toolsDependencies", []),
	])
	return tool


def init_tools_data(args, my_url):
	ctn = getCompilerToolName(args)
	tool = OrderedDict([
		('name', ctn),
		('version', args.cversion),
		('systems', [
			{
				'host': 'i686-mingw32',
				'url': my_url + "windows/" + args.cname + "-" + args.cversion + '-windows.tar.bz2',
				'archiveFileName': args.cname + "-" + args.cversion + '-windows.tar.bz2',
			},
			{
				'host': 'x86_64-apple-darwin',
				'url': my_url + 'macosx/' + args.cname + "-" + args.cversion + '-mac.tar.bz2',
				'archiveFileName': args.cname + "-" + args.cversion + '-mac.tar.bz2',
			},
			{
				'host': 'x86_64-pc-linux-gnu',
				'url': my_url + 'linux64/' + args.cname + "-" + args.cversion + '-x86_64-pc-linux-gnu.tar.bz2',
				'archiveFileName': args.cname + "-" + args.cversion + '-x86_64-pc-linux-gnu.tar.bz2',
			},
		])
	])
	return tool
