import re
import glob

print "instanceName;randomSeed;initialTemp;finalTemp;runtimeSeconds"
for f in glob.glob("./../etc/test-results/*.info"):
    with open(f) as f:
        content = f.readlines()

    valueRe = re.compile('[\d.]+', re.IGNORECASE)
    valueRS = re.compile('rs=([\d-]*)', re.IGNORECASE).search(f.name).group(1);
    valueName = re.compile('([\d_]*)\.rs', re.IGNORECASE).search(f.name).group(1);

    initS = valueRe.search(content[10]).group();
    finalS = valueRe.search(content[11]).group();
    sec = valueRe.search(content[12]).group();

    print "{0};{1};{2};{3};{4}".format(valueName, valueRS, initS, finalS, sec)
