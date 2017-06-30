#!/usr/bin/env python
#coding:utf-8
# Author: zoraida
# Created: 01/04/2014
"""
Outputs specific records from a line separated by a custom separator
"""
from coo2cfg import output_fields, output_separator
import json


def getFromDict(dataDict, mapList):
    return reduce(lambda d, k: d[k], mapList, dataDict)


def mapper(k, v):
    """
    Outputs output_files from config file separated by output_separator
    from config file
    """
    instance = json.loads(v)
    field_values = []
    for f in output_fields:
        field = getFromDict(instance, f.split('.'))
        if isinstance(field, dict):
            field_values.append(unicode(json.dumps(field), "utf-8"))
        elif isinstance(field, basestring):
            field_values.append(getFromDict(instance, f.split('.')))
        else:#list
            field_values.append(','.join(field))

    line = output_separator.join(v for v in field_values)
    yield line, ''


def runner(job):
    opts = [("inputformat", "text"),
            ("outputformat", "text")
           ]
    job.additer(mapper, reducer=None, opts=opts)


if __name__ == "__main__":
    from dumbo import main
    main(runner)
