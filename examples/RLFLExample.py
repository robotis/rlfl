"""
"""
class RLFLExample():
    def print_map(self, ll, S, T):
        """
        """
        print('\n->')
        for row in range(len(self.map)):
            for col in range(len(self.map[row])):
                if (row, col) == S:
                    print('S', end="")
                elif (row, col) == T:
                    print('T', end="")
                elif (row, col) in ll:
                    print('*', end="")
                else:
                    print(self.map[row][col], end="")
            print()
        print('<-')