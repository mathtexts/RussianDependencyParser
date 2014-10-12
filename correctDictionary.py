__author__ = 'steve'

in_block = False
remember_block = False
block = []
prev_block = []
out_file = open('../openCorpora/good_dict.opcorpora.txt', 'w')
for line in open('../openCorpora/dict.opcorpora.txt'):
    # print line
    line_parts = line.rstrip().split()
    if len(line_parts) == 1:
        in_block = True
    elif len(line_parts) == 0:
        in_block = False
        if remember_block:
            remember_block = False
            prev_block = block
            block = []
        elif block:
            print >>out_file, ''.join(block)
            block = []
            if prev_block:
                print >>out_file, ''.join(prev_block)
                prev_block = []
    elif len(line_parts) > 1 and line_parts[1].startswith('VERB,'):
        remember_block = True
    if in_block:
        block.append(line)

out_file.close()