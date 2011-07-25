"""
"""
import sys

class RLFLExample():
    def print_map(self, ll, S, T):
        """
        """
        print('\n->')
        for row in range(len(self.map)):
            print('\t', end="")
            for col in range(len(self.map[row])):
                if (row, col) == S:
                    if(sys.version_info[0] > 2):
                        print('S', end="")
                    else:
                        print('S',)
                elif (row, col) == T:
                    if(sys.version_info[0] > 2):
                        print('T', end="")
                    else:
                        print('T',)
                elif (row, col) in ll:
                    if(sys.version_info[0] > 2):
                        print('*', end="")
                    else:
                        print('*',)
                else:
                    print(self.map[row][col], end="")
            print()
        print('<-')