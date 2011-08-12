"""
"""
import sys

class RLFLExample(object):
    def print_map(self, ll, S, T):
        """
        """
        print '\n->' 
        for row in range(len(self.map)):
            sys.stdout.write('\t')
            for col in range(len(self.map[row])):
                if (row, col) == S:
                    sys.stdout.write('S')
                elif (row, col) == T:
                    sys.stdout.write('T')
                elif (row, col) in ll:
                    sys.stdout.write('*')
                else:
                    sys.stdout.write(self.map[row][col])
            print ""
        print '<-' 