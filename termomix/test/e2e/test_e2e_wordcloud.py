__author__ = 'fmrm'

import unittest
import os
import visualizations
import test

wordcloud_path = os.path.dirname(visualizations.__file__) + "/wordcloud.py"
base_dir = os.path.dirname(test.__file__) + "/.."

print ("...Initializing E2E tests in %s" % wordcloud_path)
os.chdir(base_dir)
print ("...Working directory: %s" % os.getcwd())


class EndToEndWordCloudTests(unittest.TestCase):
    def test_e2e_wordcloud_webcrawl(self):
        result = os.system("python " + wordcloud_path + " test_webcrawl")
        self.assertEqual(result, 0)

    def test_e2e_wordcloud_peru(self):
        result = os.system("python " + wordcloud_path + " test_peru")
        self.assertEqual(result, 0)

    def test_e2e_wordcloud_lleida(self):
        self.assertEqual(True, False, "Test not implemented yet")

    # Convenience method to debug main called from the tests module
    #def test_debug_wordcloud_webcrawl(self):
    #    import sys
    #    import visualizations.wordcloud as wordcloud
    #    sys.argv[1:] = ["test_webcrawl"]
    #    wordcloud.main(sys.argv[1:])

if __name__ == '__main__':
    unittest.main()
