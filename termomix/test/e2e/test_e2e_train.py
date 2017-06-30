__author__ = 'fmrm'

import unittest
import os
import classification
import test

train_path = os.path.dirname(classification.__file__) + "/train.py"
base_dir = os.path.dirname(test.__file__) + "/.."

print ("...Initializing E2E tests in %s" % train_path)
os.chdir(base_dir)
print ("...Working directory: %s" % os.getcwd())

class EndToEndLearnTests(unittest.TestCase):
    def test_e2e_learn_webcrawl(self):
        result = os.system("python " + train_path + " test_webcrawl")
        self.assertEqual(result, 0)

    def test_e2e_learn_peru(self):
        result = os.system("python " + train_path + " test_peru")
        self.assertEqual(result, 0)

    def test_e2e_learn_lleida(self):
        self.assertEqual(True, False, "Test not implemented yet")

    # Convenience method to debug main called from the tests module
    #def test_debug_learn_webcrawl(self):
    #    import sys
    #    import learn
    #    sys.argv[1:] = ["test_webcrawl"]
    #    learn.main(sys.argv[1:])

if __name__ == '__main__':
    unittest.main()
