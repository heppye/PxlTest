#!/usr/bin/env python

import sys
sys.argv.append('-b')

import ROOT
ROOT.gStyle.SetLineWidth(2)
ROOT.gStyle.SetFrameLineWidth(2)
ROOT.gStyle.SetHistLineWidth(2)
ROOT.gStyle.SetPadBorderSize(2)
ROOT.gStyle.SetCanvasBorderSize(2)
ROOT.gStyle.SetOptStat(0)
#ROOT.gStyle.SetOptFit(1)

import re
import time
import math
import numpy
#import urllib2
#import HTMLParser


class FillParser:
	def __init__(self, filename):
		self.fill_db = {}
		columns = []
		for line in open(filename, 'r'):
			entries = [re.sub(r'<.*?>', '', x).strip() for x in line.split('\t')]
			if len(columns) == 0:
				columns = entries
			else:
				assert len(columns) == len(entries)

				entries_map = {}
				for column, entry in zip(columns, entries):
					entries_map[column] = entry

				match = re.match('([0-9]+) hr ([0-9]+) min', entries_map['Duration Stable'])
				if not match: continue # not a stable beams fill
				duration = int(match.group(1))*60 + int(match.group(2))

				starttime = time.strptime(entries_map['Create Time'], '%Y.%m.%d %H:%M:%S')

				self.fill_db[int(entries_map['Fill'])] = {
					'nr': int(entries_map['Fill']),
					'delivered': float(entries_map['Delivered Lumi']),
					'duration': duration,
					'time': time.mktime(starttime),
					'runs': [int(x) for x in entries_map['Runs'].split()],
					'type': entries_map['Type']
				}

	def integrate_fills(self, begin, end):
		fill_list = []
		prev_lumi = 0.
		for nr in sorted(self.fill_db):
			fill = self.fill_db[nr]
			if fill['time'] < begin or fill['time'] > end: continue

			fill['integrated'] = prev_lumi
			prev_lumi += fill['delivered']

			fill_list.append(fill)
		return fill_list

	def get_fills_for_year(self, year):
		begin_time = time.mktime(time.strptime('%d.01.01 00:00:00' % year, '%Y.%m.%d %H:%M:%S'))
		end_time = time.mktime(time.strptime('%d.12.31 23:59:59' % year, '%Y.%m.%d %H:%M:%S'))
		return self.integrate_fills(begin_time, end_time)

class HistogramFetcher:
	def __init__(self):
		self.files = {}

	def get_era_for_run(self, run):
		ERAS = [
			('2010A', 135821, 144114),
			('2010B', 146248, 149509),
			('2011A', 160413, 173692),
			('2011B', 175832, 180296),
			('2012A', 190450, 193680),
			('2012B', 193752, 196531),
			('2012C', 198022, 203742),
			('2012D', 203894, 208686)
		]

		for era, runmin, runmax in ERAS:
			if run >= runmin and run <= runmax:
				return era

		return None
		#raise Exception('No era for run %d' % run)

	def get_filename_for_run(self, run):
		return 'collected/%s.root' % self.get_era_for_run(run)

	def get_for_run(self, run, histoname):
		era = self.get_era_for_run(run)
		if not era:
			return None

		if not era in self.files:
			self.files[era] = ROOT.TFile(self.get_filename_for_run(run))

		file = self.files[era]
		if not file or file.IsZombie():
			return None

		histo = file.Get('Histos/Run%d/%s' % (run, histoname))
		if not histo:
			return None

		return histo

	def get_for_runs(self, runs, histoname):
		histo = None
		for run in runs:
			tmp_histo = self.get_for_run(run, histoname)
			if tmp_histo:
				if histo is None:
					histo = tmp_histo.Clone()
				else:
					histo.Add(tmp_histo)
		return histo

#parser = FillParser('FillReport_1357475584627.csv')

#parser = FillParser('FillReport_1393931874387.cvs')

#parser = FillParser('FillReport_1393936368730.cvs')

parser = FillParser('FillReport_1394025326780.cvs')

for fill in parser.get_fills_for_year(2012):
	ts = time.gmtime(fill['time'])
	yday = ts.tm_yday + (ts.tm_hour * 3600. + ts.tm_min * 60. + ts.tm_sec) / (24.*60.*60.)
	print 'Fill %d (%s %g, %gpb-1): %s' % (fill['nr'], fill['type'], yday, fill['integrated'], ','.join(['%d' % x for x in fill['runs']]))
