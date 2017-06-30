#!/usr/bin/env python
# -*- coding: utf-8 -*-
# Author: fmrm
# Created: 22/07/2014
"""
"""
import logging
import xlsxwriter
from processing.text.term_statistics import get_terms_docs_stats

logging.basicConfig(level=logging.DEBUG, format='%(module)s:%(funcName)s %(asctime)s %(levelname)s %(message)s')


def create_docterm_statistics_file(path, terms, documents, freq_dtm=None, feature_names=None, image=None):
    """
    Create an Excel file with appearance of a set of terms within a set of documents
    :param path: Path to the Excel file (including extension)
    :param terms: The terms to look for
    :param documents: The set of documents where to look for the term
    :param freq_dtm: DTM if it is already calculated. Otherwise, DTM will be calculated
    :param feature_names: ndarray with the column names of the DTM (terms) if the DTM was already calculated
    """
    logging.debug("Creating excel file: %s " % path)

    headers_1 = [(2, 3, "Documents with term"), (4, 6, "Term frequency in documents"),
                 (7, 9, "Term first appearances (absolute)"), (10, 12, "Term first appearances (% of doc length)")]
    headers_2 = ["Term", "Frequency", "Total", "Percentage",
                 "Median", "Mean", "STD",
                 "Median", "Mean", "STD",
                 "Median", "Mean", "STD"]

    row_documents = 1
    row_headers = row_documents + 2
    num_headers = 2
    col_base = 1
    width_col_base = 21
    width_col_stats = 12

    # Create an new Excel file and add a worksheet.
    wb = xlsxwriter.Workbook(path)
    ws = wb.add_worksheet("Stats")

    # Define formats
    bold_fmt = wb.add_format({'bold': True})
    header_fmt = wb.add_format({'bold': True, 'align': 'center'})
    decimal_fmt = wb.add_format({'num_format': '0.##0'})
    percentage_fmt = wb.add_format({'num_format': '0.00%'})

    # Get statistics to write
    stats = get_terms_docs_stats(terms, documents, freq_dtm, feature_names)
    
    # Write number of documents (global)
    ws.write(row_documents, col_base, "Number of documents:", bold_fmt)
    ws.write_number(row_documents, col_base + 1, len(documents))

    #Set columns width
    ws.set_column(col_base, col_base, width_col_base)
    ws.set_column(col_base + 1, col_base + len(headers_2), width_col_stats)

    # Write group headers
    for header in headers_1:
        ws.merge_range(row_headers, col_base + header[0], row_headers, col_base + header[1], header[2], header_fmt)
    # Write column headers
    ws.write_row(row_headers + 1, col_base, headers_2, header_fmt)

    # Write stats
    row = row_headers + num_headers - 1
    for term in stats.keys():
        row += 1
        st = stats[term]
        ws.write(row, col_base, term)
        ws.write(row, col_base + 1, st['num_appearances'])
        ws.write(row, col_base + 2, st['num_docs_with_term'])
        ws.write(row, col_base + 3, st['pct_docs_with_term'], percentage_fmt)
        ws.write(row, col_base + 4, st['term_in_doc_median'], decimal_fmt)
        ws.write(row, col_base + 5, st['term_in_doc_mean'], decimal_fmt)
        ws.write(row, col_base + 6, st['term_in_doc_std'], decimal_fmt)
        ws.write(row, col_base + 7, st['pos_first_appearance_abs_median'], decimal_fmt)
        ws.write(row, col_base + 8, st['pos_first_appearance_abs_mean'], decimal_fmt)
        ws.write(row, col_base + 9, st['pos_first_appearance_abs_std'], decimal_fmt)
        ws.write(row, col_base + 10, st['pos_first_appearance_pct_median'], percentage_fmt)
        ws.write(row, col_base + 11, st['pos_first_appearance_pct_mean'], percentage_fmt)
        ws.write(row, col_base + 12, st['pos_first_appearance_pct_std'], decimal_fmt)

    # Add autofilter
    ws.autofilter(row_headers + 1, col_base, row_headers + 1, col_base + len(headers_2) - 1)

    # Allow to include an image with the data
    if image is not None:
        wb.add_worksheet("WC").insert_image('B2', image)

    wb.close()

    logging.debug("Created excel file: %s " % path)

